.PHONY: clean

nixrewrite: nixrewrite.cc
	g++ -std=c++11 -O3 $< -o $@

clean:
	rm -f nixrewrite
