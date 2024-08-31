all: warehouse
warehouse:
	g++ -c -g -Wall -Weffc++ -std=c++11 -Iinclude -o bin/Action.o src/Action.cpp
	g++ -c -g -Wall -Weffc++ -std=c++11 -Iinclude -o bin/Customer.o src/Customer.cpp
	g++ -c -g -Wall -Weffc++ -std=c++11 -Iinclude -o bin/main.o src/main.cpp
	g++ -c -g -Wall -Weffc++ -std=c++11 -Iinclude -o bin/Order.o src/Order.cpp
	g++ -c -g -Wall -Weffc++ -std=c++11 -Iinclude -o bin/Volunteer.o src/Volunteer.cpp
	g++ -c -g -Wall -Weffc++ -std=c++11 -Iinclude -o bin/WareHouse.o src/WareHouse.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -Iinclude -o bin/warehouse bin/Action.o bin/Customer.o bin/main.o bin/Order.o bin/Volunteer.o bin/WareHouse.o

clean:
	rm -f ./bin/*.o ./bin/warehouse