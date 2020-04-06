

#include "templates_test.h"

void Template_Tests();
extern HAL::Uart uart1;

#if 0
class BaseV
{
public:
  virtual bool Out(char* str)
  {
    printf("%s",str);
    printf("Base class \n\r");     
    return true;
  }
  
};


class DerivedV : public BaseV
{
public:
  //virtual bool Out(char* str);
  //  {    
  //    printf("%s",str);
  //    printf("Derived class \n\r");
  //    return true;
  //  }
};

#else

template< typename Derived>
class BaseV
{
public:
  bool Out(char* str)
  {
    static_cast<Derived*>(this)->OutD(str);
    printf("Base class \n\r");     
    return true;
  }
  
};


class DerivedV : public BaseV<DerivedV>
{
public:
   bool OutD(char* str)
    {    
      printf("%s",str);
      printf("Derived class \n\r");
      return true;
    }
};

class Derived1V : public BaseV<Derived1V>
{
public:
   bool OutD(char* str)
    {    
      printf("%s",str);
      printf("Derived1 class \n\r");
      return true;
    }
};
#endif
void Template_Tests()
{
  
  DerivedV DerivedV_Obj;
  Derived1V Derived1V_Obj;
  //BaseV   BaseV_Obj;
  volatile int i = sizeof(DerivedV);
  
  //BaseV_Obj.Out("Hello from ");
  DerivedV_Obj.Out((char*)"Hello from ");
  Derived1V_Obj.Out((char*)"Hello from ");
  while(1)
  {
    
  }
  
}