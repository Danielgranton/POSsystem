# POS System

A modern desktop Point of Sale (POS) system built with C++, Qt, and PostgreSQL.

This project is evolving from a terminal-based POS into a full desktop application with a graphical user interface using Qt Widgets.

---

## Features

## Authentication System

- User login system
- Admin and Cashier roles
- Hidden password input
- Role-based access

## Product Management

- Add products
- View products
- Update stock
- Remove products
- Search products

## Sales System

- Sell products
- Automatic stock deduction
- Sales history tracking
- Revenue calculation
- Receipt generation

## User Management

- Add users
- Remove users
- View users

## Alerts & Analytics

- Low stock alerts
- Daily revenue reports
- Dashboard metrics

## GUI (Qt Desktop Application)

- Login window
- Admin dashboard
- Desktop-based POS interface
- PostgreSQL integration using Qt SQL

---

## Tech Stack

| Technology | Purpose |
|---|---|
| C++ | Core application logic |
| Qt6 Widgets | Desktop GUI |
| PostgreSQL | Database |
| libpqxx | PostgreSQL C++ library |
| CMake | Build system |

---

## Project Structure

```txt
POS/GUI
├── main.cpp
├── mainwindow.cpp
├── mainwindow.h
├── mainwindow.ui
├── admindashboard.cpp
├── admindashboard.h
├── backendservice.cpp
├── backendservice.h
├── database.cpp
├── database.h
├── product.cpp
├── product.h
├── sale.cpp
├── sale.h
├── CMakeLists.txt
└── README.md
