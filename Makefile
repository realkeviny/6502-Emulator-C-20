.PHONY: prepare dependency conan_d conan_r clean

prepare:
	cmake -E rm -rf build
	cmake -E make_directory build

clean:
	cmake -E rm -rf build

dependency:
	cd build && cmake .. --graphviz=graph.dot && dot -Tpng graph.dot -o graphimage.png

conan_d: prepare
	cd build && conan install .. -s build_type=Debug -s compiler.cppstd=23 --output-folder=. --build=missing

conan_r: prepare
	cd build && conan install .. -s build_type=Release -s compiler.cppstd=23 --output-folder=. --build=missing
