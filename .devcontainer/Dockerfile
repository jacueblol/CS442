# Base image
FROM mcr.microsoft.com/devcontainers/base:noble

# Set the working directory
WORKDIR /workspace

# Install dependencies
RUN DEBIAN_FRONTEND="noninteractive" apt update && apt upgrade -y && apt install -y \
  build-essential \
  clang \
  dos2unix \
  emacs \
  gdb \
  git \
  libboost-all-dev \
  libfmt-dev \
  libreadline-dev \
  nano \
  neovim \
  openssh-client \
  perl \
  valgrind \
  vim \
  zip \
  && apt clean && rm -rf /var/lib/apt/lists/*

# Sleep forever (Not necessary when this image is used as a dev container, but is necessary when
# this image is used standalone)
CMD sleep infinity
