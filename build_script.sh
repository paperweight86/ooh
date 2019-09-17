cpp_files=$1

cpp_files="$cpp_files"| xargs

cpp_inc="-I../Tat/inc -I../Ooh/inc"

cpp_inc="$cpp_inc"| xargs

fixing_errors=""
#fixing_errors="-Wno-everything"

link_files="../Tat/tat.a -lpthread"
link_paths=""

clang++ -shared -undefined dynamic_lookup -fPIC -o $2 $fixing_errors -msse4.1 -std=c++1y -DLINUX -m64 -g3 $cpp_files $cpp_inc -I./ $link_files $link_paths
#chmod +x test.Ooh
#ar -rcs Ruin.elf *.o