#!/usr/bin/env ruby

$infile = ''


if ARGV.size() < 1
    puts './inline_headers <input code file>'
    exit
else
    $infile = ARGV[0]
end

$outfile = 'inlined-' + $infile
puts "Infile=#{$infile}"
puts "Outfile=#{$outfile}"

# read infile into array of lines
$lines = []
File.open($infile, "r").each_line do |line|
    $lines << line
end

begin
    #open file to write to
    $fout = File.open($outfile, "w")

    # write lines to outfile
    # until //---REPLACE-BEGIN--- is found
    $line_iter = 0
    while $lines[$line_iter] != "//---REPLACE-BEGIN---\n"
        $fout.write($lines[$line_iter])
        $line_iter += 1
    end
    $line_iter +=1 #skip begin line
    
    # now replace alle following headers with the actual code
    # until //---REPLACE-END--- is found
    while $lines[$line_iter] != "//---REPLACE-END---\n"
        headerfile_name = $lines[$line_iter].match(/"([^"]*)"/).to_s
        headerfile_name.gsub!(/\A"|"\Z/, '')
        
        #open header file and write contents to outfile
        File.readlines(headerfile_name).each do |line| 
	#TODO: remove lines with manual includes
            $fout.write(line)
        end 

        $line_iter += 1
    end
    $line_iter +=1 #skip end line
    
    #transfer all remaining code from infile to outfile
    while $lines[$line_iter]
        $fout.write($lines[$line_iter])
        $line_iter += 1
    end
        
rescue IOError => e


#close outfile
ensure
    $fout.close unless $fout == nil
end

puts "Inlining complete."
