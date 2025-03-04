#include "_rebuild/rebuild.h"

int main(int argc, char **argv) {
  std::string cflags = "-Iinclude";
  system("mkdir -p build");
  rebuild_targets.push_back(
      Target::create("build/libyhook.so", {"build/yhook_pic.o"},
                     "gcc -fPIC -shared -o build/libyhook.so #DEPENDS"));
  rebuild_targets.push_back(Target::create("build/libyhook.a",
                                           {"build/yhook.o"},
                                           "ar rcs build/libyhook.a #DEPENDS"));

  rebuild_targets.push_back(
      CTarget::create("build/yhook.o", {"src/yhook.c"},
                      "gcc -c -o build/yhook.o #DEPENDS " + cflags));
  rebuild_targets.push_back(
      CTarget::create("build/yhook_pic.o", {"src/yhook.c"},
                      "gcc -fPIC -c -o build/yhook_pic.o #DEPENDS" + cflags));

  rebuild_targets.push_back(CTarget::create(
      "build/yhook_example1", {"src/example1.c", "build/libyhook.a"},
      "gcc -o build/yhook_example1 #DEPENDS " + cflags));

  rebuild_targets.push_back(CTarget::create(
      "build/yhook_example2", {"src/example2.cpp", "build/libyhook.a"},
      "g++ -o build/yhook_example2 #DEPENDS " + cflags));

  return 0;
}
