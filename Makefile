debug = 

switch_chess.exe: ./build/utils.o ./build/core.o ./build/assets.o ./build/anim_text.o ./build/chess.o ./build/uci_engine.o \
				./build/scene_game.o ./build/switch_chess.o ./build/scene_game_init.o ./build/scene_home.o ./build/autoplay.o
	
	g++ $(debug) -o switch_chess.exe ./build/utils.o ./build/core.o ./build/assets.o ./build/anim_text.o ./build/chess.o ./build/uci_engine.o \
				./build/scene_game.o ./build/scene_game_init.o ./build/switch_chess.o ./build/scene_home.o ./build/autoplay.o \
				-IC:/Users/padmadevd/programming/cyg_libs/include -I.\
				-LC:/Users/padmadevd/programming/cyg_libs/libs -lraylib -luser32 -lgdi32 -lshell32

./build/utils.o: utils.cpp
	g++ $(debug) -c utils.cpp -o ./build/utils.o -IC:/Users/padmadevd/programming/cyg_libs/include -I.

./build/core.o: core.cpp
	g++ $(debug) -c core.cpp -o ./build/core.o -IC:/Users/padmadevd/programming/cyg_libs/include -I.

./build/assets.o: assets.cpp
	g++ $(debug) -c assets.cpp -o ./build/assets.o -IC:/Users/padmadevd/programming/cyg_libs/include -I.

./build/anim_text.o: anim_text.cpp
	g++ $(debug) -c anim_text.cpp -o ./build/anim_text.o -IC:/Users/padmadevd/programming/cyg_libs/include -I.

./build/chess.o: chess.cpp
	g++ $(debug) -c chess.cpp -o ./build/chess.o -IC:/Users/padmadevd/programming/cyg_libs/include -I.

./build/uci_engine.o: uci_engine.cpp
	g++ $(debug) -c uci_engine.cpp -o ./build/uci_engine.o -IC:/Users/padmadevd/programming/cyg_libs/include -I.

./build/autoplay.o: autoplay.cpp
	g++ $(debug) -c autoplay.cpp -o ./build/autoplay.o -IC:/Users/padmadevd/programming/cyg_libs/include -I.

./build/scene_game.o: scene_game.cpp
	g++ $(debug) -c scene_game.cpp -o ./build/scene_game.o -IC:/Users/padmadevd/programming/cyg_libs/include -I.

./build/scene_game_init.o: scene_game_init.cpp
	g++ $(debug) -c scene_game_init.cpp -o ./build/scene_game_init.o -IC:/Users/padmadevd/programming/cyg_libs/include -I.

./build/scene_home.o: scene_home.cpp
	g++ $(debug) -c scene_home.cpp -o ./build/scene_home.o -IC:/Users/padmadevd/programming/cyg_libs/include -I.

./build/switch_chess.o: switch_chess.cpp
	g++ $(debug) -c switch_chess.cpp -o ./build/switch_chess.o -IC:/Users/padmadevd/programming/cyg_libs/include -I.

run: switch_chess.exe
	./switch_chess.exe

debug: switch_chess.exe
	gdb ./switch_chess.exe