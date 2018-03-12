
Added Ashish Srivastava. Press backspace to remove.

Skip to content
Using Gmail with screen readers

1 of 1,181
 
Docker file of rupee wallet. 
Inbox
x 

Ashish Srivastava <me@ashishsrivastav.com>
Attachments4:21 PM (8 hours ago)
to me 
Please find attachment for docker file.

my rupee address is
RNwMDVADXPain5thgwwkpvxp21ocpHg4ge


2 Attachments
0.4 GB (2%) of 15 GB used
Manage
Terms - Privacy
Last account activity: 7 minutes ago
Open in 1 other location  Details

FROM ubuntu:16.04

MAINTAINER Ashish Srivastava <me@ashishsrivastav.com>

ENV SRC_DIR /usr/local/src/rupeed
ENV CONF_DIR /root/.rupee/

ADD rupee.conf $CONF_DIR

RUN set -x \
  && buildDeps=' \
      ca-certificates \
      cmake \
      g++ \
      git \
      libboost1.58-all-dev \
      libssl-dev \
      make \
      pkg-config \
  ' \
  && apt-get -qq update \
  && apt-get -qq --no-install-recommends install $buildDeps

  RUN apt-get install qt4-qmake libqt4-dev build-essential libboost-dev libboost-system-dev \
   libboost-filesystem-dev libboost-program-options-dev libboost-thread-dev \
    libssl-dev libdb++-dev libminiupnpc-dev -y


RUN git clone https://github.com/rupeedigitalassets/RUPEE.git $SRC_DIR

WORKDIR $SRC_DIR/src


RUN make -f makefile.unix

EXPOSE 10459
EXPOSE 10460

CMD rupeed
Dockerfile
Displaying rupee.conf.
