dependency:
	cd build && cmake .. --graphviz=graph.dot && dot -Tpng graph.dot -o graphimage.png

prepare_linux:
	rm -rf build
	mkdir build

prepare_windows:
	if exist build rmdir /s /q build
	mkdir build

# conan_d:
# 	rm -rf build
# 	mkdir build
# 	cd build && conan install .. -s build_type=Debug -s compiler.cppstd=20 -output_folder=. --build missing 

# conan_r:
# 	rm -rf build
# 	mkdir build
# 	cd build && conan install .. -s build_type=Release -s compiler.cppstd=20 -output_folder=. --build missing

conan_d:
	if exist build rmdir /s /q build
	mkdir build
	cd build && conan install .. -s build_type=Debug -s compiler.cppstd=23 --output-folder=. --build missing


conan_r:
	if exist build rmdir /s /q build
	mkdir build
	cd build && conan install .. -s build_type=Release -s compiler.cppstd=23 --output-folder=. --build missing


