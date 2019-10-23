# Pull base image.
FROM ubuntu:18.04

# Install.
RUN \
  sed -i 's/# \(.*multiverse$\)/\1/g' /etc/apt/sources.list && \
  apt update && \
  apt install -y make && \
  apt install -y g++ && \
  apt install -y git

RUN \
  git clone https://github.com/Radpc/bd_tp2.git /root/tp2/ && \
  rm -rf /var/lib/apt/lists/*

# Set environment variables.
ENV HOME /root
ENV PS1 'database >' 
ENV PATH="/root/tp2/bin/:${PATH}"

# Define working directory.
WORKDIR /root/tp2/csv_files

# Build
RUN make -C ../
RUN make -C ../ clean 

# Change name
RUN echo PS1="\"[DATABASE] > \"" >> ~/.bashrc

# Define default command.
CMD clear && bash
