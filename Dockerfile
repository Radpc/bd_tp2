# Pull base image.
FROM ubuntu:18.04

# Install.
RUN \
  sed -i 's/# \(.*multiverse$\)/\1/g' /etc/apt/sources.list && \
  apt update && \
  apt install -y make && \
  apt install -y g++ && \
  rm -rf /var/lib/apt/lists/*

# Set environment variables.
ENV HOME /root
ENV PS1 'database >' 

# Define working directory.
WORKDIR /root

# Copy files
COPY . $HOME/

# Build
RUN make
RUN make clean 

# Change name
RUN echo PS1="\"[DATABASE] > \"" >> ~/.bashrc

# Define default command.
CMD clear && bash
