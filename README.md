This is a distributed URL shortening system developed in C. It is a server to generate short, unique URLs by hashing the original URL and encoding it into a shortened format.
Integrated SQLite to store and manage the mappings between original URLs and their shortened counterparts, ensuring efficient retrieval and updates.
Set up a Dockerfile for easy deployment and management of the URL shortener service, including automatic building and configuration of the C server and SQLite database.
