#ifndef INTERP_HPP
#define INTERP_HPP

template<class T>
class interp
{
public:
    interp(T _start, T _target);
    interp(T _start, T _target, float _mul);

    void setStart(const T _start) {m_start = _start;}
    void setTarget(const T _target) {m_target = _target;}

    T getStart() const {return m_start;}
    T getTarget() const {return m_target;}

    void update();
    void get() const {return cur;}

    void lerp(const float _interp) {return _interp * m_start + (1.0f - _interp) * m_target;}
private:
    T m_start;
    T m_target;
    T m_cur;

    float m_mul;
};

template<class T>
interp<T>::interp(T _start, T _target)
{
    m_start = _start;
    m_target = _target;
    m_mul = 0.0f;
}

template<class T>
interp<T>::interp(T _start, T _target, float _mul)
{
    m_start = _start;
    m_target = _target;
    m_mul = _mul;
}


#endif
