FROM node:8

WORKDIR /home/dietpi/git/iot-projekt/backend/control-center

COPY package*.json ./

RUN npm install

RUN npm install pm2 -g

COPY . .

EXPOSE 8080

CMD ["pm2-runtime", "test_provider/provider_test_color.js"]

