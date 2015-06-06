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

package ghtv.lang;

import java.io.PrintStream;
import java.io.InputStream;
import java.util.Properties;

public class System
{
    public static final InputStream in = null/*java.lang.System.in*/;
    public static final PrintStream out = /*new ghtv.io.aux.ProxyPrintStream
                                            (*/java.lang.System.out/*)*/;
    public static final PrintStream err = null/*java.lang.System.err*/;

    public static long currentTimeMillis()
    {
        return java.lang.System.currentTimeMillis();
    }
    public static void arraycopy(Object src, int src_position, Object dst,
                                 int dst_position, int length)
    {
        java.lang.System.arraycopy(src, src_position, dst, dst_position, length);
    }
    public static int identityHashCode(Object x)
    {
        return java.lang.System.identityHashCode(x);
    }
    public static Properties getProperties()
    {
        return java.lang.System.getProperties();
    }
    public static void setProperties(Properties props)
    {
    }
    public static String getProperty(String key)
    {
        return java.lang.System.getProperty(key);
    }
    public static String getProperty(String key, String def)
    {
        return java.lang.System.getProperty(key, def);
    }
    public static String getenv(String name)
    {
        return java.lang.System.getenv(name);
    }
    public static void exit(int status)
    {
    }
    public static void gc()
    {
    }
    public static void runFinalization()
    {
    }
    public static void runFinalizersOnExit(boolean value)
    {
    }
    public static void load(String filename)
    {
    }
    public static void loadLibrary(String libname)
    {
    }
    public static String mapLibraryName(String libname)
    {
        return null;
    }
}
