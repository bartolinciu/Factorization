english:
	g++ main.cpp -Wall -Wextra -pedantic -o factorization.exe -std=gnu++11 -lGdi32 -ggdb -Wl,-subsystem,windows 
polish: 
	g++ main.cpp -Wall -Wextra -pedantic -o factorization.exe -std=gnu++11 -lGdi32 -ggdb -Wl,-subsystem,windows -DPOLISH