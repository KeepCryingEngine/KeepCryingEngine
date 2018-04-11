using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KeepCryingEngine.BehaviourTree
{
    public class DataContext
    {
        private Dictionary<string, object> m_data = new Dictionary<string, object>();

        public bool Get<T>(string key, out T value)
        {
            bool ret;
            if (ret = m_data.TryGetValue(key, out object objectValue))
            {
                value = (T)objectValue;
            }
            else
            {
                value = default(T);
            }

            return ret;
        }

        public void Set<T>(string key, T value)
        {
            m_data[key] = value;
        }

        public void Remove(string key)
        {
            m_data.Remove(key);
        }
    }
}
