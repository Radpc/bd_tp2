# Pull base image.
FROM ubuntu:18.04

# Install.
RUN apt update && \
  apt-get install -y make && \
  apt-get install -y g++ && \
  apt-get install -y git

RUN git clone https://github.com/Radpc/bd_tp2.git /root/tp2/

COPY ./csv_files/entrada.csv /root/tp2/csv_files/entrada.csv

COPY ./questoes/ /root/tp2/questoes/

# Set environment variables.
ENV HOME /root
ENV PATH="/root/tp2/bin/:${PATH}"

# Define working directory.
WORKDIR /root/tp2/csv_files

# Build
RUN make -C ../
RUN make -C ../ clean 

# Define default command.
CMD clear && bash
