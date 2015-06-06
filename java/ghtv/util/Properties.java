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

package ghtv.util;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.util.Hashtable;
import java.util.Enumeration;

public class Properties extends Hashtable
{
    protected Properties defaults;

    public Properties() {}
    public Properties(Properties defaults) {}

    public Object setProperty(String key, String value)
    {
        assert false;
        return null;
    }
    public void load(InputStream inStream) throws IOException
    {
    }
    public void save(OutputStream out, String header)
    {
        assert false;
    }
    public void store(OutputStream out, String header) throws IOException
    {
        assert false;
    }
    public String getProperty(String key)
    {
        System.out.println("Searching property key: " + key);
        return null;
    }
    public String getProperty(String key, String defaultValue)
    {
        assert false;
        return null;
    }
    public Enumeration propertyNames()
    {
        assert false;
        return null;
    }
    public void list(PrintStream out)
    {
        assert false;
    }
    public void list(PrintWriter out)
    {
        assert false;
    }
}
