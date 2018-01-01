// Compiler allows definition of undeclared template class members.

template<class T> class X
{

};

// No error! Neither of these were declared in X, yet compiler allows them to be defined.
template<class T> int X<T>::dummy;

template<class T> int X<T>::scummy();
