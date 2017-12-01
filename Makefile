## Instrucciones para deployar:
## Escribir make deploy en la consola.
## Cuando pida contraseña de root, proporcionarla.
## Cuando pida user de github, proporcionarlo.

## Cada vez que se haga "make deploy" el make automáticamente
## bajará lo que no esté bajado y compilará lo no compilado.
## Si se desea rehacer el proceso, escribir "sudo make clean", lo que reiniciará todo.

deploy: so-commons-library readline biblioteca dataNode fileSystem master worker yama

so-commons-library:
	$(call mostrarTitulo,$@)
	git clone https://github.com/sisoputnfrba/so-commons-library ../so-commons-library
	cd ../so-commons-library; sudo make install

readline:
	$(call mostrarTitulo,$@)
	sudo apt-get install libreadline6 libreadline6-dev

biblioteca:
	$(call mostrarTitulo,$@)
	cd biblioteca-propia/Debug; make all
	mkdir /usr/include/biblioteca
	sudo cp -u ./biblioteca-propia/Debug/libbiblioteca-propia.so /usr/lib/libbiblioteca-propia.so
	sudo cp -u ./biblioteca-propia/biblioteca/*.h /usr/include/biblioteca

dataNode:
	$(call mostrarTitulo,$@)
	cd dataNode/Debug; make all

fileSystem:
	$(call mostrarTitulo,$@)
	cd fileSystem/Debug; make all

master:
	$(call mostrarTitulo,$@)
	cd master/Debug; make all

worker:
	$(call mostrarTitulo,$@)
	cd worker/Debug; make all

yama:
	$(call mostrarTitulo,$@)
	cd yama/Debug; make all

clean:
	$(call mostrarTitulo,$@)
	rm -rf ../so-commons-library
	rm -rf /usr/include/biblioteca
	rm -rf /usr/lib/libbiblioteca-propia.so

define mostrarTitulo
	@echo
	@echo "########### $(1) ###########################################"
endef
