# Makefile (starter) for Mission Incomputable

MAKE = make

# These targets do not produce real "targets"
.PHONY: all clean start-proxy stop-proxy

############## default: make all libs and programs ##########

all: 
	@echo "Making everything..."
	if [ `uname` != Linux ] ; then $(MAKE) -C field-agent ; fi
	if [ `uname` == Linux ] ; then $(MAKE) -C guide-agent ; fi
	if [ `uname` == Linux ] ; then $(MAKE) -C game-server ; fi

########################### proxy ###########################

# Start the proxy running - add flags as desired. Options: -all, -fa2gs, -gs2fa
start-proxy: proxy/proxy
	@cd proxy; node proxy

# Kill any running instances of the proxy.
stop-proxy:
	@pkill -f 'node proxy -all'

# Use npm to install all proxy dependencies (only need to do this initially).
install-proxy-deps:
	@cd proxy; npm install

########################### proxy ###########################

clean:
	@echo "Cleaning everything..."
	if [ `uname` != Linux ] ; then $(MAKE) -C field-agent clean ; fi
	$(MAKE) -C guide-agent clean
	$(MAKE) -C game-server clean
	rm -f *~ *.o
