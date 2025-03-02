FROM ubuntu:22.04

# downloads gcc and sqlite
RUN apt-get update && apt-get install -y \
    gcc \
    sqlite3 \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

# set up directory in container
WORKDIR /app

# copy code into container
COPY url.c /app/url.c

# Compile
RUN gcc url.c -o url -lsqlite3

# runs compiled code
CMD ["./url"]
