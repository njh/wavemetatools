#!/usr/bin/perl
#
#  bwf2mp3.pl
#  Convert a MPEG encoded WAV to MP3
#  Tab size: 4
#
#  Nicholas Humfrey <njh@aelius.com>
#  Last Modified: 31/5/01
#

use MP3::Tag;

my $waveInfoCmd = '/usr/local/bin/bwfinfo';
my $ReadBufferSize = 32768;

# Check for a filename on the command line
foreach $filename (@ARGV) {
	processFile($filename);
}

print "Done.\n";

exit;





# Process an mp3 file
sub processFile {
	my($fileName) = @_;

	print "Processing file: $filename\n";
	print "Getting information about the WAVE file...";
	my $fullCmd = "$waveInfoCmd ".quotemeta($fileName);
	my @waveInfo = split(/\n/, `$fullCmd`);
	print "\n";
	
	# Make sure it is an MPEG file
	if (getField("AudioFormat", @waveInfo) ne "MPEG") {
		warn "WAV file is not encoded as MPEG.\n";
		return;
	}
	
	# Find the file offset and length
	my $fileOffset = getField("AudioDataSeek", @waveInfo);
	my $fileLength = getField("AudioDataSize", @waveInfo);
	if ($fileOffset eq "" || $fileLength eq "") {
		warn "Cannot find either audio data length or offset in WAVE file.\n";
		return;
	}
	
	# Get information about the track
	my $title = getField("Title", @waveInfo);
	my $artist = getField("Artist", @waveInfo);
	my $album = getField("Album", @waveInfo);
	my $year = getField("Year", @waveInfo);
	my $comment = getField("Comment", @waveInfo);
	my $genre = getField("Genre", @waveInfo);
	my $publisher = getField("Publisher", @waveInfo);
	my $bpm = getField("BPM", @waveInfo);
	my $copyright = getField("Copyright", @waveInfo);
	
	# Work out the name of the new file
	$fileName =~ /^(.*)\/([^\/]*)$/;
	my $path = $1;
	$path .= "/" if ($path ne "");
	#print "Path is: $path\n";
	my $artistSpare = 12-length($artist); $artistSpare = 0 if ($artistSpare < 0);
	my $titleSpare = 12-length($title); $titleSpare = 0 if ($titleSpare < 0);
	my $newFilename = $path.substr($artist, 0, 12+$titleSpare)." - ".
				  substr($title, 0, 12+$artistSpare).".mp3";
	print "Filename: $newFilename\n";
	
	# Copy the audio data out of the WAV and into the MP3
	print "Copying audio data...";
	if (open(READ, "<$fileName")) {
		seek(READ, $fileOffset, 0) || warn "Failed to seek to start of audio data: $!\n";
		if (open(WRITE, ">$newFilename")) {
			while($fileLength) {
				if ($fileLength < $ReadBufferSize) { $readLen = $fileLength; }
				else { $readLen = $ReadBufferSize; }
				$fileLength -= read READ, $buf, $readLen;
				print WRITE $buf;
			}
			close(WRITE);
		} else { warn "Failed to open output file: $!"; }
	} else { warn "Failed to open input file: $!"; }
	close(READ);
	print "\n";
	
	# Now set the ID3 tags on the new file
	print "Setting ID3 tags...";	
	$mp3 = MP3::Tag->new($newFilename);
	$id3v2 = $mp3->newTag("ID3v2");
	$id3v2->add_frame("TIT2", $title) if (defined $title);
	$id3v2->add_frame("TPE1", $artist) if (defined $artist);
	$id3v2->add_frame("TALB", $album) if (defined $album);
	$id3v2->add_frame("TYER", $year) if (defined $year);
	$id3v2->add_frame("COMM", "ENG", "$PROGRAM_NAME", $comment) if (defined $comment);
	$id3v2->add_frame("TCON", $genre) if (defined $genre);
	$id3v2->add_frame("TPUB", $publisher) if (defined $publisher);
	$id3v2->add_frame("TBPM", $bpm) if (defined $bpm);
	$id3v2->add_frame("TCOP", $copyright) if (defined $copyright);
	$id3v2->write_tag();
	$mp3->close();
	print "\n";
}


# Extract a single field from the array
sub getField {
	my($fieldName, @dataLines) = @_;
	my($line) = grep(/^$fieldName /, @dataLines);
	$line =~ /^$fieldName (.*)$/;

	return $1;
}

