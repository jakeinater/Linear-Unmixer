# spectralILU
This small cli program is a proof of concept designed to read multiplexed data from fluorescent immuno-assays by determining different analyte concentrations using spectral imaging and linear unmixing. Hopefully this can be developed into an accurate and open source tool useful in biological assays and eventually be applied to highthroughput measurements.

You can find the meat of the program in the src directory. Simply switch into the directory and use the makefile to build the program unmix.

Note, you need to install [gsl](https://www.gnu.org/software/gsl/) to use this program. Ensure the library location is added to your path using ldconfig or try adding the library location to the end of your LD_LIBRARY_PATH.

You can find the software I used in the spectrometer [here](https://www.theremino.com/en/downloads/automation)

Please feel free to leave constructive criticism and advice! You can email me at zhaojake14@gmail.com or jake.zhao@mail.mcgill.ca.

[heres](https://youtube.com/playlist?list=PLWZJllFn0WSnx-8hw8GBx-BZZ9quGwBL9) a demo!
