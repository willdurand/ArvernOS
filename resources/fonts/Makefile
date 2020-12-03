FONTLIST = Inconsolata/static/Inconsolata-Black.ttf
FONTS = $(FONTLIST:%.ttf=%.psf)

$(FONTS):
	$(eval FROM=$@)
	$(eval FROM=$(FROM:%.psf=%.ttf))
	$(eval TO=$@)
	$(eval TO=$(TO:%.psf=%.bdf))
	otf2bdf -r 72 -p 16 -c C $(FROM) | sed -e "s/AVERAGE_WIDTH.*/AVERAGE_WIDTH 70/" > $(TO) 
	bdf2psf $(TO) /usr/share/bdf2psf/standard.equivalents /usr/share/bdf2psf/fontsets/Uni2.512 512 $@
	rm $(TO)
	cp $@ ../../initrd/fonts/
	rm $@

all: $(FONTS)

clean:
	find . -name "*.bdf" -type f -delete
	find . -name "*.psf" -type f -delete
	rm ../../initrd/fonts/*
	mkdir ../../initrd/fonts

.PHONY: clean all
