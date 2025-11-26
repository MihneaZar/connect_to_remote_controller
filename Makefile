make: test.hpp connect_to_remote_controller.cpp controller_console/controller_console.cpp combat_zone/combat_zone.cpp destructor/destructor.cpp connect_to_remote_controller.hpp controller_console/controller_console.hpp combat_zone/combat_zone.hpp destructor/destructor.hpp
	g++ test.hpp connect_to_remote_controller.cpp controller_console/controller_console.cpp combat_zone/combat_zone.cpp destructor/destructor.cpp connect_to_remote_controller.hpp controller_console/controller_console.hpp combat_zone/combat_zone.hpp destructor/destructor.hpp -o connect_to_remote_contoller -lssl -lcrypto

# at least for now, make run fails because of md5
run: connect_to_remote_controller
	./connect_to_remote_controller sysadmin .ignore

.PHONY: clean

clean:
	rm connect_to_remote_contoller