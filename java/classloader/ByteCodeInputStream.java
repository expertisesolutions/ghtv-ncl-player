/* (c) Copyright 2011-2014 Felipe Magno de Almeida
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

package ghtv.classloader;

import java.io.InputStream;
import java.io.IOException;
import java.io.ByteArrayOutputStream;
import java.nio.ByteBuffer;
import java.util.LinkedList;
import java.util.ArrayList;
import java.util.List;

class ByteCodeInputStream extends InputStream
{
    InputStream source;
    LinkedList<Integer> replace = new LinkedList<Integer>();
    int minor_version;
    int major_version;
    int current_constant_pool;
    int constant_pool_count;
    ArrayList<String> java_replaces = new ArrayList<String>();
    List<Integer> java_string = java.util.Arrays.asList
        (new Integer[]{new Integer('j'), new Integer('a'), new Integer('v')
                       , new Integer('a'), new Integer('/')});

    public enum State
    {
        magic, constant_pool_setup, constant_pool, rest
    }

    State state = State.magic;

    public ByteCodeInputStream(InputStream s)
    {
        source = s;
        minor_version = major_version = current_constant_pool = 0;

        java_replaces.add(new String("io/FileInputStream"));
        //java_replaces.add(new String("lang/System"));
        //java_replaces.add(new String("util/Properties"));
    }

    public int magic_read() throws IOException
    {
        int x1 = source.read();
        replace.addLast(source.read());
        replace.addLast(source.read());
        replace.addLast(source.read());
        state = State.constant_pool_setup;
        return x1;
    }

    private Integer[] make_replaces(Integer[] s1)
    {
        System.out.println("make_replaces");
        List<Integer> source = java.util.Arrays.asList(s1);
        ArrayList<Integer> result = new ArrayList<Integer>();
        int i = 0, size = source.size();
        while(i != size)
        {
            boolean match = false;
            if(size - i >= java_string.size())
            {
                List<Integer> sub_java = source.subList(i, i+java_string.size());
                if(sub_java.equals(java_string))
                {
                    int j = 0;
                    while(j != java_replaces.size())
                    {
                        String cmp = java_replaces.get(j);
                        System.out.println("Trying " + cmp);
                        int comp_size = cmp.length();
                        if(size - i - java_string.size() >= comp_size)
                        {
                            int m = i + java_string.size();
                            int k = 0;
                            while(k != comp_size)
                            {
                                if(cmp.charAt(k) == source.get(k + m))
                                    ++k;
                                else
                                    break;
                            }
                            if(k == comp_size)
                            {
                                System.out.println("Found a match");
                                result.add((int)'g');
                                result.add((int)'h');
                                result.add((int)'t');
                                result.add((int)'v');
                                result.add((int)'/');
                                for(int l = 0; l != cmp.length(); ++l)
                                    result.add((int)cmp.charAt(l));
                                i = k + m;
                                match = true;
                                break;
                            }
                        }

                        ++j;
                    }
                }
            }
            if(!match)
                result.add(source.get(i++));
        }
        return result.toArray(new Integer[result.size()]);
    }

    public int constant_pool_read() throws IOException
    {
        ++current_constant_pool;
        if(current_constant_pool == constant_pool_count-1)
        {
            state = State.rest;
        }
        assert(current_constant_pool != constant_pool_count);

        {
            int tag = source.read();
            System.out.println(current_constant_pool + " tag: " + tag);
            switch(tag)
            {
            case 5: // Long
            case 6: // Double
                ++current_constant_pool;
                replace.addLast(source.read());
                replace.addLast(source.read());
                replace.addLast(source.read());
                replace.addLast(source.read());
            case 9: // Fieldref
            case 10: // Methodref
            case 11: // InterfaceMethodref
            case 3: // Integer
            case 4: // Float
            case 12: // NameAndType
                replace.addLast(source.read());
                replace.addLast(source.read());
            case 7: // Class
            case 8: // String
                replace.addLast(source.read());
                replace.addLast(source.read());
                break;
            case 1: // Utf8
                ByteBuffer buffer = ByteBuffer.allocate(2);
                int c = source.read();
                replace.addLast(c);
                buffer.put((byte)c);
                c = source.read();
                replace.addLast(c);
                buffer.put((byte)c);
                buffer.flip();
                int size = buffer.getShort();
                //System.out.println("size: " + size);
                assert(size >= 0);
                Integer[] string = new Integer[size];
                //System.out.print("Found string constant: ");
                for(int i = 0; i != size; ++i)
                {
                    string[i] = source.read();
                    //System.out.write((char)string[i].intValue());
                }    
                //System.out.println("");

                Integer[] r = make_replaces(string);
                for(int i = 0; i != r.length; ++i)
                    replace.addLast(r[i]);
                break;
            default:
                //System.out.println("Unknown tag " + tag);
            }
            return tag;
        }
    }

    public int constant_pool_setup_read() throws IOException
    {
        ByteBuffer buffer = ByteBuffer.allocate(10);
        int first_c = source.read();
        buffer.put((byte)first_c);
        int c = source.read();
        replace.addLast(c);
        buffer.put((byte)c);
        buffer.flip();

        minor_version = buffer.getShort();
        buffer.flip();
        System.out.println("minor: " + minor_version);

        c = source.read();
        replace.addLast(c);
        buffer.put((byte)c);
        c = source.read();
        replace.addLast(c);
        buffer.put((byte)c);
        buffer.flip();

        major_version = buffer.getShort();
        buffer.flip();
        System.out.println("major: " + major_version);

        c = source.read();
        replace.addLast(c);
        buffer.put((byte)c);
        c = source.read();
        replace.addLast(c);
        buffer.put((byte)c);

        buffer.flip();
        constant_pool_count = buffer.getShort();

        //System.out.println("constant_pool_count: " + constant_pool_count);

        state = State.constant_pool;
        return first_c;
    }
    
    public int read() throws IOException
    {
        if(replace.size() != 0)
        {
            int x = replace.getFirst();
            replace.removeFirst();
            return x;
        }
        switch(state)
        {
        case magic:
            return magic_read();
        case constant_pool_setup:
            return constant_pool_setup_read();
        case constant_pool:
            return constant_pool_read();
        default:
            return source.read();
        }
    }
}
