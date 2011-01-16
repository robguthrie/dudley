#!/usr/bin/ruby
#a ruby script to generate a cpp header and implementation with getters and setters defined
#usage as follows gencpp.rb ClassName QString:firstProp qint64:secondProp
require 'rubygems'
require 'active_support/core_ext'

def header_filename(class_name)
  class_name.downcase+".h"  
end

def body_filename(class_name)
  class_name.downcase+".cpp"
end

def def_name(name)
  name.upcase+"_H"
end

def m_name(name)
  "m_"+name.camelize(:lower)
end

def get_name(name)
  name.camelize(:lower)
end

def set_name(name)
  "set"+name.camelize
end

def get_declaration(type, name)
  "    #{type} #{get_name(name)}() const;\n"
end

def set_declaration(type, name)
  "    void #{set_name(name)}(#{type} #{name});\n"
end

def get_definition(class_name, type, name)
  "#{type} #{class_name}::#{get_name(name)}()\n{\n    return #{m_name(name)};\n}\n\n"
end

def set_definition(class_name, type, name)
  "void #{class_name}::#{set_name(name)}(#{type} #{name})\n{\n    #{m_name(name)} = #{name};\n}\n\n"
end

def q_property(type, name)
  "    Q_PROPERTY(#{type} #{name} READ #{get_name(name)} WRITE #{set_name(name)});\n"
end

def property(type, name)
  "    #{type} #{m_name(name)};\n"
end

def header_file(class_name, properties)
  r = ""
  r << "#ifndef #{def_name(class_name)}\n"
  r << "#define #{def_name(class_name)}\n"
  r << "class #{class_name} : public QObject\n{\n    Q_OBJECT\n\npublic:\n"

  properties.each do |pair|
    type, name = pair
    r << q_property(type, name)
  end
  
  properties.each do |pair|
    type, name = pair
    r << get_declaration(type, name)
  end
  
  properties.each do |pair|
    type, name = pair
    r << set_declaration(type, name)
  end
  r << "\nprivate:\n"
  
  properties.each do |pair|
    type, name = pair
    r << property(type, name)
  end
  r << "};\n"
  r << "#endif //#{def_name(class_name)}\n"
  r
end

def body_file(class_name, properties)
  r = "#include \"#{header_filename(class_name)}\"\n"
  properties.each do |pair|
    type, name = pair
    r << get_definition(class_name, type, name)
  end
  
  properties.each do |pair|
    type, name = pair
    r << set_definition(class_name, type, name)
  end
  r << "\n"
  r
end


if ARGV.size == 0
  puts "usage: gencpp Car QString:name int:numDoors"
else
  properties = []
  class_name = ""
  ARGV.each do |term|
    if class_name.empty?
      class_name = term
    else
      properties << term.split(':')
    end
    puts "header_file: #{header_filename(class_name)}\n"
    puts header_file(class_name, properties)
    puts "\n body_file: #{body_filename(class_name)}\n"
    puts body_file(class_name, properties)
  end
end