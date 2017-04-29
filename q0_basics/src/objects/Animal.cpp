#include "Animal.h"
#include <string>   // even though Animal.h will automatically include <string>,
                    // it is usually a good idea to include here too.  Its #ifdef guards
                    // will prevent it from actually loading twice.

// Define all our class functions

// Animal constructor
Animal::Animal(std::string name, int age)
  : _name(name), _age(age) // initialze the internal _name and _age to the provided name and age
{
  // nothing else to do here
}

// Animal destructor
Animal::~Animal() {
  // delete any dynamic memory here (our animal doesn't have any yet)
}

// Animal Speak
std::string Animal::Speak() {
  return "...";  // we don't know what to say
}

// Class of referring type (not overridden, but can be hidden)
std::string Animal::Class() {
  return "Animal";
}

// Name "accessor", returns the animal's name
std::string Animal::Name() {
  return _name;
}

// Age "accessor", returns the animal's age
int Animal::Age() {
  return _age;
}