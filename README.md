WAVE Meta Tools
===============

A collection of roughly coded tools to manipulate WAVE files, 
with an emphasis on the metadata used by the radio industry.

The perl scripts require the MP3::Tag perl module:
* http://tagged.sf.net/
* http://search.cpan.org/~thogee/tagged/


wavemetainfo
------------
display the chunks contained in a WAVE file (rfc822 style)


waveunwrap
----------
unwrap a WAVE file. Takes the 'data' chunk from a WAVE file and copys 
it to an output file. Works well for extacting MPEG Audio, but may be 
useful for other WAVE file formats too.


bsiwave_to_mpeg
---------------
Perl script to convert a BSI style WAVE file and its metadata 
to an MPEG Audio file with the metadata stored in ID3 tags.

