make: test.hpp connect_to_remote_controller.cpp controller_console/controller_console.cpp destructor/destructor_actions.cpp connect_to_remote_controller.hpp controller_console/controller_console.hpp destructor/destructor_actions.hpp
	g++ test.hpp connect_to_remote_controller.cpp controller_console/controller_console.cpp destructor/destructor_actions.cpp connect_to_remote_controller.hpp controller_console/controller_console.hpp destructor/destructor_actions.hpp -o connect_to_remote_contoller -lssl -lcrypto

# at least for now, make run fails because of md5
run: connect_to_remote_controller
	./connect_to_remote_controller sysadmin .ignore

.PHONY: clean

clean:
	rm connect_to_remote_contoller