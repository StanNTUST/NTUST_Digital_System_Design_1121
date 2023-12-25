1.Run file in the linux
  (1)update
    sudo apt install g++ build-essential
    sudo apt install vim
  (2)run
    g++ -c fileName.cpp -o fileName.o
    ex: g++ -c robdd.cpp -o robdd.o
    
    g++ fileName.o -o fileName
    ex: g++ robdd.o -o robdd

    ./fileName fileName.dot fileName.pla
    ex: ./robdd case1.pla case1.dot

2.output the picture
  dot -T png fileName.dot > fileName.png
  ex: dot -T png robdd.dot > output.png

3.Precautions
  (1) I only judge the first letter is function if it is 0, 1 or -
  (2) Without (.p number) this line is valid
  (3) I provide dot file and image file for you to check