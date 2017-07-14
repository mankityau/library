#include <iostream>  // for printing stuff
#include "cpen333/util.h"
#include "Animal.h"  // to reference an Animal
#include "Cat.h"     // to create a Cat
#include "Dog.h"     // to create a Dog

int main() {

  // allocate a cat and dog on the "heap"
  Cat* garfield = new Cat("Garfield", 39);  // Garfield is a really old Cat
  Dog* wishbone = new Dog("Wishbone", 22);  // Wishbone is also super old now :S

  // pointer to an existing animal
  Animal* pet;

  // What do the animals say?
  std::cout << "Testing Cat/Dog pointers" << std::endl;
  std::cout << "  " << garfield->Name() << " the " << garfield->Class() << " says " << garfield->Speak() << std::endl;
  std::cout << "  " << wishbone->Name() << " the " << wishbone->Class() << " says " << wishbone->Speak() << std::endl;

  std::cout << "Testing Animal pointer" << std::endl;
  pet = garfield;
  std::cout << "  " << pet->Name() << " the " << pet->Class() << " says " << pet->Speak() << std::endl;
  // Did it print what you expected?

  // Trying out functions unique to Cat and Dog
  std::cout << wishbone->Class() << "s " << wishbone->Drool() << " and "
            << garfield->Class() << "s " << garfield->Rule() << std::endl;

  // Forcing it to get at the parent class' Speak() method
  // this is WAY more than what you need to know
  std::cout << "  Forcing " << pet->Name() << " to say " << pet->Animal::Speak() << std::endl;

  // free heap memory
  delete wishbone;
  delete garfield;

  cpen333::pause();
  return 0;
}