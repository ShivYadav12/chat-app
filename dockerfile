FROM gcc:latest
WORKDIR /app
COPY . .
RUN g++ -std=c++17 -pthread -o server server.cpp
RUN g++ -std=c++17 -pthread -o client client.cpp
EXPOSE 54000
CMD ["./server"]
