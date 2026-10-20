#!/usr/bin/env bash
# a script that install some

sudo apt-get -y update
sudo apt-get install -y certbot
sudo systemctl stop nginx
sudo service haproxy stop
sudo certbot certonly --standalone -d www.rezzcode.tech
