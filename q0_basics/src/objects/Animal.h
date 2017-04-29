#ifndef ANIMAL_H  // ifdef "guard" to prevent definitions from loading multiple times
#define ANIMAL_H

#include <string>  // c++ string (which differs from a c-style string that is just an array of chars)

// new "animal" base class
class Animal {
 // private, hidden stuff
 private:
  // for private members, I like to use a prefixed _ to indicate that they are private
  std::string _name;  // string is in the "std" namespace
                      // name of the animal
  int _age;           // age of the animal

 // public, accessible stuff
 public:

  Animal(std::string name, int age);    // constructor that passes in name and age
  virtual ~Animal();                    // destructor, destroys any internal data

  // "virtual" functions can be overridden by sub-classes
  virtual std::string Speak();          // returns a noise

  // non-virtual functions are not "overridden", but can be "hidden" by sub-classes
  std::string Class();

  // "accessor" methods for private member variables
  // so that outside world can see what they are, but cannot change them
  std::string Name();                   // returns our name
  int Age();                            // returns our age

};

#endif