# POS

Simple POS backend in C++ using PostgreSQL and `libpqxx`.

## Backend Setup

The backend reads database settings from environment variables. If a variable is not set, the app falls back to the current default value in code.

Supported variables:

- `POS_DB_NAME` default: `pos_system`
- `POS_DB_USER` default: `pos_user`
- `POS_DB_PASSWORD` default: `possystem`
- `POS_DB_HOST` default: local PostgreSQL socket
- `POS_DB_PORT` default: `5433`

Example:

```bash
export POS_DB_NAME=pos_system
export POS_DB_USER=pos_user
export POS_DB_PASSWORD=possystem
export POS_DB_HOST=127.0.0.1
export POS_DB_PORT=5433
```

## Build

```bash
cd backend
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp database.cpp product.cpp sale.cpp -lpqxx -lpq -o pos
```

## Run

```bash
cd backend
./pos
```
