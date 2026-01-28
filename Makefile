output := Terminesweeper
cflags := -Wall -Wextra

$(output): main.c
	gcc $(cflags) main.c -o $(output)

run: $(output)
	./$(output)

clean:
	rm -f $(output)
