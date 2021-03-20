using System;
using System.Collections.Generic;

namespace BtreeCs
{
    class Program
    {
        const int n = 10000000; //n queries: 3/4 of them insert, 1/4 search
        static void GenerateRandomTests(List<int> content, List<int> seq, List<int> op)
        {
            var rnd = new Random();
            for (int i = 0; i < n; i++)
            {
                int choice = rnd.Next(0, int.MaxValue) % 4;
                if (choice < 3 || content.Count == 0)
                {
                    int val = rnd.Next(0, int.MaxValue);
                    seq.Add(val);
                    op.Add(0);
                    content.Add(val);
                }
                else
                {
                    int val;
                    if (rnd.Next(0, int.MaxValue) % 5 != 1)
                        val = content[rnd.Next(0, int.MaxValue) % content.Count];
                    else
                        val = rnd.Next(0, int.MaxValue);
                    seq.Add(val);
                    op.Add(1);
                }
            }
        }
        static void Main(string[] args)
        {
            var content = new List<int>();
            var seq = new List<int>();
            var op = new List<int>();
            GenerateRandomTests(content, seq, op);

            Console.WriteLine("Start!");

            int sum1 = 0;
            var btree = new BtreeSimple<int>(); 
            var watch = System.Diagnostics.Stopwatch.StartNew();
            for (int i = 0; i < n; i++)
            {
                if (op[i] == 0)
                    btree.Insert(seq[i]);
                else if (op[i] == 1)
                    sum1 += btree.Search(seq[i]) ? 1 : 0;
            }
            watch.Stop();
            Console.WriteLine($"duration B-tree = {watch.ElapsedMilliseconds} ms");

            int sum2 = 0;
            var set = new SortedSet<int>();
            watch = System.Diagnostics.Stopwatch.StartNew();
            for (int i = 0; i < n; i++)
            {
                if (op[i] == 0)
                    set.Add(seq[i]);
                else if (op[i] == 1)
                    sum2 += set.Contains(seq[i]) ? 1 : 0;
            }
            watch.Stop();
            Console.WriteLine($"duration set    = {watch.ElapsedMilliseconds} ms");

            if (sum1 != sum2)
                Console.WriteLine($"Sums are different: {sum1} {sum2}");
        }
    }
}