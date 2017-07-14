#include "Dog.h"
#include <string>

Dog::Dog(std::string name, int age) : Animal(name, age) { // pass the name and age to the parent constructor
  // initialize anything else unique to Dog
}

Dog::~Dog() {
  // clear any dynamic memory unique to Dog
}

// Override "Species" to return Dog
std::string Dog::Speak() {
  return "woof";
}

// Class of referring type (not overridden, but can be hidden)
std::string Dog::Class() {
  return "Dog";
}

// Unique method to Dog
std::string Dog::Drool() {
  // ewww, the slobber is everywhere
  return "drool";
}