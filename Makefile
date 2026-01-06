make: *.cpp *.hpp controller_console/*  
	g++ *.cpp *.hpp controller_console/* -o connect_to_remote_controller -lssl -lcrypto

# at least for now, make run fails because of md5
run: connect_to_remote_controller
	./connect_to_remote_controller sysadmin .ignore

.PHONY: clean

clean:
	rm connect_to_remote_contoller