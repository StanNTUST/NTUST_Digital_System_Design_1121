1.Run file in the linux
  (1)update
    sudo apt install g++ build-essential
    sudo apt install vim
  (2)run
    g++ -c fileName.cpp -o fileName.o
    ex: g++ -c B11130038.cpp -o minimization.o
    
    g++ fileName.o -o fileName
    ex: g++ minimization.o -o minimization

    ./fileName fileName.pla fileName.pla
    ex: ./minimization case1.pla output1.pla

3.Precautions
  (1) I only judge the first letter is function if it is 0, 1 or -
  (2) Without (.p number) this line is valid