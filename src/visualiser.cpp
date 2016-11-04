#include <iostream>

#include <algorithm>

#include <ngl/NGLInit.h>
#include <ngl/Mat3.h>
#include <ngl/Mat4.h>
#include <ngl/Random.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>

#include <time.h>

#include "common.hpp"
#include "physicsvars.hpp"
#include "shape.hpp"
#include "visualiser.hpp"
#include "util.hpp"

visualiser::visualiser()
{
	m_lmb = false;
	m_mmb = false;

	m_tZoom = 5.0f;
	m_cZoom = 5.0f;

	std::cout << "p1\n";
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
		errorExit("SDL initialisation failed");

	SDL_DisplayMode best;
	best.w = 0;
	best.h = 0;

	for(int i = 0; i < SDL_GetNumVideoDisplays(); ++i)
	{
		SDL_DisplayMode temp;
		SDL_GetCurrentDisplayMode(i, &temp);
		if(temp.w * temp.h > best.w * best.h)
			best = temp;
	}

	m_w = best.w;
	m_h = best.h;


	m_window = SDL_CreateWindow("mGen",
															0, 0,
															m_w, m_h,
															SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS );

	if(!m_window)
		errorExit("Unable to create window");

	//Setting up the GL attributes before they can be used
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	m_gl_context = SDL_GL_CreateContext( m_window );

	if(!m_gl_context)
		errorExit("Unable to create GL context");

	makeCurrent();
	SDL_GL_SetSwapInterval(0);

	ngl::NGLInit::instance();

	ngl::Random * rnd = ngl::Random::instance();
	rnd->setSeed( time(NULL) );

	createShaderProgram( "blinn", "MVPUVNVert", "blinnFrag" );
	createShaderProgram( "bufferLight", "screenquadVert", "bufferLightFrag" );

	std::cout << "p1.5\n";

	m_framebuffer.initialise(m_w, m_h);

	m_framebuffer.addTexture( "diffuse", GL_RGBA, GL_RGBA, GL_COLOR_ATTACHMENT0 );
	m_framebuffer.addTexture( "normal", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT1 );
	m_framebuffer.addTexture( "position", GL_RGBA, GL_RGBA16F, GL_COLOR_ATTACHMENT2 );
	m_framebuffer.addTexture( "radius", GL_RED, GL_RED, GL_COLOR_ATTACHMENT3 );
	std::cout << "ERROR " <<  glGetError() << '\n';
	m_framebuffer.addDepthAttachment("depth");

	m_framebuffer.activeColourAttachments(
	{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3}
				);

	if(!m_framebuffer.checkComplete())
		errorExit( "Error! Framebuffer failed!\n" );

	m_framebuffer.unbind();

	GLint v = 0;
	glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &v );

	//Create and reserve light buffer.
	m_lights.reserve(MAX_LIGHTS);
	glGenBuffers(1, &m_lightbuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, m_lightbuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(light) * MAX_LIGHTS, &m_lights[0].m_pos.m_x, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glViewport(0, 0, m_w, m_h);

	m_cam = ngl::Camera(
				ngl::Vec3(0, 0, 1),
				ngl::Vec3(0, 0, 0),
				ngl::Vec3(0, 1, 0)
				);

	m_cam.setShape(
				90.0f,
				(float)m_w / (float)m_h,
				0.5f,
				2048.0f
				);

	ngl::VAOPrimitives * prim = ngl::VAOPrimitives::instance();

	for(int i = 0; i < 4; ++i)
	{
		std::string name ("sphereLOD" + std::to_string( i ));
		prim->createSphere(
					name,
					1.0f,
					pow(2.0f, i + 2)
					);
		m_meshes.push_back( name );
	}
	std::reverse( m_meshes.begin(), m_meshes.end() );

	createVAO(
				"screenquad",
	{
					ngl::Vec4(-1.0f, -1.0f, 0.0f, 1.0f),
					ngl::Vec4(-1.0f, 1.0f, 0.0f, 1.0f),
					ngl::Vec4(1.0f, 1.0f, 0.0f, 1.0f),
					ngl::Vec4(1.0f, -1.0f, 0.0f, 1.0f)
				},
	{
					ngl::Vec2(0.0, 0.0),
					ngl::Vec2(0.0, 1.0),
					ngl::Vec2(1.0, 1.0),
					ngl::Vec2(1.0, 0.0)
				}
				);

	createVAO(
				"floor",
	{
					ngl::Vec4(-1000.0f, -10.0f, -1000.0f, 1.0f),
					ngl::Vec4(-1000.0f, -10.0f, 1000.0f, 1.0f),
					ngl::Vec4(1000.0f, -10.0f, -1000.0f, 1.0f),
					ngl::Vec4(1000.0f, -10.0f, 1000.0f, 1.0f)
				},
	{
					ngl::Vec2(0.0, 0.0),
					ngl::Vec2(0.0, 1.0),
					ngl::Vec2(1.0, 1.0),
					ngl::Vec2(1.0, 0.0)
				}
				);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	clear();
	swap();
	hide();
}

visualiser::~visualiser()
{
	SDL_DestroyWindow( m_window );
}

void visualiser::addPoint(const ngl::Vec3 &_vec, const std::string &_name, const float _mass)
{
	sphere node(_vec, _mass);
	node.setName(_name);

	m_nodes.push_back( node );
}

void visualiser::broadPhase(ngl::Vec3 _min, ngl::Vec3 _max, const std::vector<sphere *> &_nodes, unsigned short _lvl)
{
	std::vector<sphere *> outNodes;
	unsigned short count = 0;

	for(auto &i : _nodes)
	{
		if(sphereAABBIntersect(_min, _max, i->getPos(), i->getRadius()))
		{
			count++;
			outNodes.push_back( i );
		}
	}

	if(count <= 4 or _lvl > 3)
	{
		m_partitions.push_back( outNodes );
		/*ngl::BBox b (
										_min.m_x,
										_max.m_x,
										_min.m_y,
										_max.m_y,
										_min.m_z,
										_max.m_z
										);

				b.draw();*/
	}
	//Subdivide otherwise.
	else
	{
		ngl::Vec3 dim = _max - _min;
		dim /= 2.0f;

		for(int i = 0; i < 2; ++i)
			for(int j = 0; j < 2; ++j)
				for(int k = 0; k < 2; ++k)
				{
					ngl::Vec3 newMin = _min + ngl::Vec3( dim.m_x * i, dim.m_y * j, dim.m_z * k);
					broadPhase(
								newMin,
								newMin + dim,
								outNodes,
								_lvl + 1
								);
				}
	}
}

GLuint visualiser::createBuffer1f(std::vector<float> _vec)
{
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,
							 sizeof(float) * _vec.size(),
							 &_vec[0],
			GL_STATIC_DRAW
			);
	return buffer;
}

GLuint visualiser::createBuffer2f(std::vector<ngl::Vec2> _vec)
{
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,
							 sizeof(ngl::Vec2) * _vec.size(),
							 &_vec[0].m_x,
			GL_STATIC_DRAW
			);
	return buffer;
}

GLuint visualiser::createBuffer3f(std::vector<ngl::Vec3> _vec)
{
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,
							 sizeof(ngl::Vec3) * _vec.size(),
							 &_vec[0].m_x,
			GL_STATIC_DRAW
			);
	return buffer;
}

GLuint visualiser::createBuffer4f(std::vector<ngl::Vec4> _vec)
{
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,
							 sizeof(ngl::Vec4) * _vec.size(),
							 &_vec[0].m_x,
			GL_STATIC_DRAW
			);
	return buffer;
}

void visualiser::createShaderProgram(const std::string _name, const std::string _vert, const std::string _frag)
{
	ngl::ShaderLib * slib = ngl::ShaderLib::instance();

	slib->createShaderProgram(_name);
	slib->attachShader(_vert, ngl::ShaderType::VERTEX);
	slib->attachShader(_frag, ngl::ShaderType::FRAGMENT);

	slib->loadShaderSource(_vert, "shaders/" + _vert + ".glsl");
	slib->loadShaderSource(_frag, "shaders/" + _frag + ".glsl");

	slib->compileShader(_vert);
	slib->compileShader(_frag);

	slib->attachShaderToProgram(_name, _vert);
	slib->attachShaderToProgram(_name, _frag);

	slib->linkProgramObject(_name);
}

void visualiser::createVAO(const std::string &_id, std::vector<ngl::Vec4> _verts)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Generate a VBO
	GLuint vertBuffer = createBuffer4f( _verts );
	setBufferLocation( vertBuffer, 0, 3 );

	std::pair<std::string, GLuint> p (_id, vao );
	m_vaos.insert( p );
}

void visualiser::createVAO(const std::string &_id, std::vector<ngl::Vec4> _verts, std::vector<ngl::Vec2> _UVs)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Generate a VBO
	GLuint vertBuffer = createBuffer4f( _verts );
	setBufferLocation( vertBuffer, 0, 4 );

	GLuint uvBuffer = createBuffer2f( _UVs );
	setBufferLocation( uvBuffer, 1, 2 );

	std::pair<std::string, GLuint> p (_id, vao );
	m_vaos.insert( p );
}

void visualiser::drawSpheres()
{
	m_framebuffer.bind();
	m_framebuffer.activeColourAttachments({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3});

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	clear();

	m_lights.clear();

	ngl::VAOPrimitives * prim = ngl::VAOPrimitives::instance();
	ngl::ShaderLib * slib = ngl::ShaderLib::instance();

	slib->use( "blinn" );

	for(auto &i : m_nodes.m_objects)
	{
		slib->setRegisteredUniform("baseColour", ngl::Vec4( i.getColour() * i.getLuminance() ));
		slib->setRegisteredUniform("radius", i.getRadius());
		//std::cout << "drawing sphere at " << i.m_x << ", " << i.m_y << ", " << i.m_z << '\n';
		//m_trans.reset();

		//float dist = (i.getPos() * m_V).m_z;

		ngl::Vec3 pos = i.getPos()/* + ngl::Vec3(0.0f, sin(g_TIME + (i.m_x * i.m_z) / 512.0f), 0.0f)*/;

		m_scale.scale( i.getRadius(), i.getRadius(), i.getRadius() );
		m_trans.translate( pos.m_x, pos.m_y, pos.m_z );
		loadMatricesToShader();

		//dist /= 32.0f;

		/*size_t index = static_cast<size_t>(std::floor(dist));
				index = clamp(index, size_t(0), m_meshes.size() - 1);*/
		prim->draw( m_meshes[0] );
		//prim->draw( m_meshes[index] );

		if(i.getLuminance() > 0.05f)
			m_lights.push_back( {i.getPos(), i.getColour(), i.getLuminance() * 0.1f} );
	}

	slib->setRegisteredUniform("baseColour", ngl::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	m_trans.translate( -m_camCLook.m_x, -m_camCLook.m_y, -m_camCLook.m_z );
	/*m_lights.push_back(
		{
										-m_camCLook,
										ngl::Vec3(1.0f, 1.0f, 1.0f),
										4.0f
								}
								);*/
	loadMatricesToShader();

	prim->draw( m_meshes[0] );

	glBindBuffer(GL_UNIFORM_BUFFER, m_lightbuffer);
	GLvoid * dat = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(dat, &m_lights[0].m_pos.m_x, sizeof(light) * std::min( m_lights.size(), static_cast<size_t>(MAX_LIGHTS)));
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void visualiser::finalise()
{
	m_framebuffer.unbind();
	//m_framebuffer.activeColourAttachments({GL_COLOR_ATTACHMENT0});

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	clear();

	ngl::ShaderLib * slib = ngl::ShaderLib::instance();

	slib->use("bufferLight");

	glBindVertexArray(m_vaos["screenquad"]);

	GLuint id = slib->getProgramID("bufferLight");

	m_framebuffer.bindTexture(id, "diffuse", "diffuse", 0);
	m_framebuffer.bindTexture(id, "normal", "normal", 1);
	m_framebuffer.bindTexture(id, "position", "position", 2);
	m_framebuffer.bindTexture(id, "radius", "radius", 3);

	GLuint lightBlockIndex = glGetUniformBlockIndex( id, "lightBuffer" );
	GLuint index = 1;
	glBindBufferBase(GL_UNIFORM_BUFFER, index, m_lightbuffer);
	glUniformBlockBinding(id, lightBlockIndex, index);

	slib->setRegisteredUniform( "activeLights", static_cast<int>(std::min( m_lights.size(), static_cast<size_t>(MAX_LIGHTS) )) );

	ngl::Vec4 camPos = m_V * m_cam.getEye() - m_camCLook;
	slib->setRegisteredUniform( "camPos", ngl::Vec3( camPos.m_x, camPos.m_y, camPos.m_z ) );

	//std::cout << "CamPos " << camPos.m_x << ", " << camPos.m_y << ", " << camPos.m_z << '\n';

	glDrawArraysEXT(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);

	SDL_GL_SwapWindow(m_window);
}

void visualiser::loadMatricesToShader()
{
	ngl::ShaderLib * slib = ngl::ShaderLib::instance();

	//ngl::Mat3 normalMat = MV;
	//normalMat.inverse();

	ngl::Mat4 M = m_scale * m_trans;
	ngl::Mat4 MVP = M * m_VP;

	slib->setRegisteredUniform( "M", M );
	slib->setRegisteredUniform( "MVP", MVP );
	//slib->setRegisteredUniform( "normalMat", normalMat );
}

void visualiser::mouseDown(SDL_Event _event)
{
	switch(_event.button.button)
	{
	case SDL_BUTTON_LEFT:
		m_mouseOrigin = getMousePos();
		m_mousePos = m_mouseOrigin;
		m_lmb = true;
		break;
	case SDL_BUTTON_MIDDLE:
		m_mouseOrigin = getMousePos();
		m_mousePos = m_mouseOrigin;
		m_mmb = true;
		break;
	default:
		break;
	}
}

void visualiser::mouseUp(SDL_Event _event)
{
	switch(_event.button.button)
	{
	case SDL_BUTTON_LEFT:
		m_mouseOrigin = getMousePos();
		m_mousePos = m_mouseOrigin;
		m_lmb = false;
		break;
	case SDL_BUTTON_MIDDLE:
		m_mouseOrigin = getMousePos();
		m_mousePos = m_mouseOrigin;
		m_mmb = false;
		break;
	default:
		break;
	}
}

void visualiser::mouseWheel(int _dir)
{
	m_tZoom = clamp( m_tZoom - 6.0f * _dir, 0.0f, F_MAX );
}

void visualiser::setBufferLocation(GLuint _buffer, int _index, int _size)
{
	glEnableVertexAttribArray(_index);
	glBindBuffer(GL_ARRAY_BUFFER, _buffer);
	glVertexAttribPointer( _index, _size, GL_FLOAT, GL_FALSE, 0, 0 );
}

void visualiser::narrowPhase()
{
	for(size_t i = 0; i < m_partitions.size(); ++i)
	{
		for(auto &a : m_partitions[i])
		{
			for(auto &b : m_partitions[i])
			{
				if(a == b)
					continue;

				ngl::Vec3 normal = b->getPos() - a->getPos();

				float ar = a->getRadius();
				float br = b->getRadius();

				float dist = normal.lengthSquared();
				if( dist > sqr(ar + br) )
					continue;

				dist = sqrt(dist);
				normal /= dist;

				float penetration = (ar + br) - dist;
				ngl::Vec3 toMove = normal * penetration;
				//toMove = ngl::Vec3(0.0f, 100000.0f, 0.0f);

				float aim = a->getInvMass();
				float bim = b->getInvMass();
				float sumMass = aim + bim;

				float addMul = 0.0f;//1.0f - g_BALL_PENETRATION_LENIENCY;
				a->addPos( addMul * (-toMove * aim / sumMass) );
				b->addPos( addMul * (toMove * bim / sumMass) );

				ngl::Vec3 rv = a->getVel() - b->getVel();
				float separation = rv.dot( normal );

				if(separation < 0.0f) continue;

				float force = -g_COLLISION_ENERGY_CONSERVATION * separation;
				force /= sumMass;

				ngl::Vec3 impulse = force * normal;
				impulse.m_z = 0.0f;

				a->addVel( aim * impulse );
				b->addVel( -bim * impulse );

				if(force < -0.1f)
				{
					force *= 16.0f;

					a->addLuminance(-force * aim / sumMass);
					b->addLuminance(-force * bim / sumMass);
				}
			}
		}
	}
}

void visualiser::update(const float _dt)
{
	if( m_lmb )
	{
		m_mousePos = getMousePos();
		m_tMouseRotation += m_mousePos - m_mouseOrigin;

		m_mouseOrigin = m_mousePos;
	}
	else if( m_mmb )
	{
		m_mousePos = getMousePos();
		ngl::Vec2 diff = m_mousePos - m_mouseOrigin;

		m_rot.transpose();
		m_camTLook += m_rot.getRightVector() * diff.m_x / 8.0f;
		m_camTLook -= m_rot.getUpVector() * diff.m_y / 8.0f;

		m_mouseOrigin = m_mousePos;

		//std::cout << completeViewMat.getUpVector().m_x << ", " << completeViewMat.getUpVector().m_y << ", " << completeViewMat.getUpVector().m_z << '\n';
	}

	m_cMouseRotation += (m_tMouseRotation - m_cMouseRotation) / 16.0f;

	//std::cout << "Mouse down, " << m_mouseRotation.m_x << ", " << m_mouseRotation.m_y << '\n';

	//Do rotations
	ngl::Mat4 pitch;
	ngl::Mat4 yaw;

	pitch.rotateX( m_cMouseRotation.m_y );
	yaw.rotateY( m_cMouseRotation.m_x );

	m_rot = yaw * pitch;

	m_cZoom += (m_tZoom - m_cZoom) / 16.0f;

	m_cam.setEye( ngl::Vec3(0.0, 0.0, 1.0) * m_cZoom );

	m_camCLook += (m_camTLook - m_camCLook ) / 16.0f;

	m_camTrans.setPosition(m_camCLook);

	m_V = m_camTrans.getMatrix() * m_rot * m_cam.getViewMatrix();
	m_VP = m_V * m_cam.getProjectionMatrix();

	//TEST
	for(auto &i : m_nodes.m_objects)
	{
		ngl::Vec3 origin = i.getPos();

		/*if(origin.length() > 512.0f)
						m_nodes.m_velocities[i] = -m_nodes.m_velocities[i];*/

		//m_nodes.m_velocities[i] *= 0.1f;

		/*if(origin.length() > 256.0f)
						std::cout << "origin pre " << origin.m_x << ", " << origin.m_y << ", " << origin.m_z << '\n';*/
		//Loop through connection.
		for(auto &id : (*i.getConnections()))
		{
			sphere * target = m_nodes.getByID( id );

			float sumRad = i.getRadius() + target->getRadius();
			float sumMass = i.getInvMass() + target->getInvMass();

			//Get distance.
			ngl::Vec3 dir = target->getPos() - origin;
			float dist = dir.length();

			/*if(dist < 64.0f)
								m_nodes.m_velocities[i] *= dist / 64.0f;*/

			float mrad = sumRad * g_BALL_STICKINESS_RADIUS_MULTIPLIER;
			if(dist > mrad)
			{
				dir /= pow(dist, g_GRAVITY_ATTENUATION);
				//Add forces to both current node and connection node (connections are one-way so we must do both here).
				i.addVel( dir * (i.getInvMass() / sumMass) );
			}
			else
				i.setVel( i.getVel() * (1.0f - g_BALL_STICKINESS) * (dist / mrad) );


			//target->addVel( -dir * (target->getInvMass()) );
			//m_nodes.addVel( index, -dir * (m_nodes.m_masses[index] / sumMass) );
		}
		//std::cout << "origin post " << origin.m_x << ", " << origin.m_y << ", " << origin.m_z << "\n\n";
	}

	m_partitions.clear();

	std::vector<sphere *> initNodes;
	initNodes.reserve( m_nodes.size() );

	std::vector<ngl::Vec3> points;
	points.reserve( m_nodes.size() );

	for(auto &i : m_nodes.m_objects)
	{
		initNodes.push_back( &i );
		points.push_back( i.getPos() );
	}

	std::pair<ngl::Vec3, ngl::Vec3> initBox = lim( points );

	broadPhase( initBox.first, initBox.second, initNodes, 0 );
	narrowPhase();

	for(auto &i : m_nodes.m_objects)
	{
		i.setVel( i.getVel() * (1.0f - g_AMBIENT_FRICTION) );
		i.update( _dt );
	}
}

