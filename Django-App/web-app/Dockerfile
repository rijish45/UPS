FROM python:3
ENV PYTHONUNBUFFERED 1
RUN mkdir /webapp
COPY . /webapp
WORKDIR /webapp
RUN pip3 install -r requirements.txt