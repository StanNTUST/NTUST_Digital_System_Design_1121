1.Run file in the linux
  (1)update
    sudo apt install g++ build-essential
    sudo apt install vim
  (2)run
    g++ -c fileName.cpp -o fileName.o
    ex: g++ -c B11130038.cpp -o smin.o
    
    g++ fileName.o -o fileName
    ex: g++ smin.o -o smin

    ./fileName fileName.kiss fileName.kiss fileName.dot
    ex: ./smin mycase.kiss myresult.kiss myresult.dot

2.output the picture
  dot -T png fileName.dot > fileName.png
  ex: dot -T png mycase.dot > mycase.png