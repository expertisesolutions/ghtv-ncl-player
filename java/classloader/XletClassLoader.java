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

import java.net.URL;
import java.io.InputStream;
import java.io.ByteArrayOutputStream;

class XletClassLoader extends ClassLoader
{
    private URL url;

    {
        ClassLoader.registerAsParallelCapable();
    }

    XletClassLoader(URL url)
    {
        this.url = url;
    }

    protected Class findClass(String name) throws ClassNotFoundException
    {
        System.out.println("findClass " + name);
        try
        {
            return ClassLoader.getSystemClassLoader().loadClass(name);
        }
        catch(ClassNotFoundException e)
        {
            System.out.println("Failed bootstrap findClass with " + name);
        }
        {
            System.out.println("Couldn't find class name: " + name);
            String file = name.replace('.', '/').concat(".class");
            System.out.println("findClass file: " + file);
            try
            {
                URL class_url = new URL(url.toExternalForm() + "/" + file);
                System.out.println("findClass url: " + class_url.toString());
                InputStream in = new ByteCodeInputStream(class_url.openStream());
                System.out.println("findClass 1");
                ByteArrayOutputStream bos = new ByteArrayOutputStream();
                int next = in.read();
                while (next > -1) {
                    bos.write(next);
                    next = in.read();
                }
                System.out.println("findClass 2");
                bos.flush();
                System.out.println("findClass 3");
                byte[] result = bos.toByteArray();
                System.out.println("findClass 4");
                return defineClass(name, result, 0, result.length);
            }
            catch(Exception e)
            {
                System.out.println("Exception was thrown while loading byte code. Trying super.findClass");
                // e.printStackTrace();
                throw new ClassNotFoundException("Not found", e);
            }
        }
    }
}
