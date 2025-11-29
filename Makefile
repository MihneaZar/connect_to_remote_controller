make: *.cpp *.hpp controller_console/* combat_zone/* destructor/* venator/* timer/* 
	g++ *.cpp *.hpp controller_console/* combat_zone/* destructor/* venator/* timer/* -o connect_to_remote_contoller -lssl -lcrypto

# at least for now, make run fails because of md5
run: connect_to_remote_controller
	./connect_to_remote_controller sysadmin .ignore

.PHONY: clean

clean:
	rm connect_to_remote_contoller