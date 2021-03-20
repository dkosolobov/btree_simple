using System;

namespace BtreeCs
{
	class Bnode<Tkey> where Tkey : IComparable<Tkey>
	{
		const int b = 16;   // sizeof(key) * b = ~64 (длина кеш линии)
		public byte Count = 0;
		public Tkey[] Keys = new Tkey[2 * b + 1];  // Keys[2*b] не используется (он нужен для упрощения реализации)
		public Bnode<Tkey>[] Kids = null;
		public Bnode(bool leaf)
		{   // Kids[2*b+1] не используется (он нужен для упрощения реализации)
			Kids = leaf? null : new Bnode<Tkey>[2 * b + 2]; 
		}
		public bool Search(Tkey key)
		{
			int i = 0;  // Array.FindIndex и BinarySearch заметно медленнее
			while (i < Count && Keys[i].CompareTo(key) < 0)
				i++;    // на коротких массивах линейный поиск быстрее бинарного
			if (i < Count && Keys[i].Equals(key))
				return true;
			return Kids != null && Kids[i].Search(key);
		}
		public (Tkey, Bnode<Tkey>) InsertAtPos(int pos, Tkey key, Bnode<Tkey> nodeAfterKey)
		{
			Array.Copy(Keys, pos, Keys, pos + 1, Count - pos);
			Keys[pos] = key;  // "раздвигаем" Keys и kids, чтобы вставить key и node_after_key
			if (Kids != null)
			{
				Array.Copy(Kids, pos + 1, Kids, pos + 2, Count - pos);
				Kids[pos + 1] = nodeAfterKey;
			}
			if (++Count <= 2 * b)
				return (default(Tkey), null);
			var split = new Bnode<Tkey>(Kids == null);
			Count = split.Count = b;  // в node сейчас 2b+1 ключей; node.Keys[b] "пойдёт наверх"
			var median = Keys[b];  // ключ Keys[b] не входит в узел и "идёт наверх"
			Array.Copy(Keys, b + 1, split.Keys, 0, b);
			Array.Fill(Keys, default(Tkey), b, b + 1);
			if (Kids != null)
			{
				Array.Copy(Kids, b + 1, split.Kids, 0, b + 1);
				Array.Fill(Kids, null, b + 1, b + 1);
			}
			return (median, split); 
		}
		public (Tkey, Bnode<Tkey>) Insert(Tkey key)
		{
			int pos = 0;  // Array.FindIndex и BinarySearch заметно медленнее
			while (pos < Count && Keys[pos].CompareTo(key) < 0)
				pos++;    // на коротких массивах линейный поиск быстрее бинарного
			Bnode<Tkey> overflow_node = null;
			if (Kids != null)
				(key, overflow_node) = Kids[pos].Insert(key);
			if (Kids == null || overflow_node != null)
				return InsertAtPos(pos, key, overflow_node);
			return (default(Tkey), null);
		}
	}
	class BtreeSimple<Tkey> where Tkey : IComparable<Tkey>
	{
		private Bnode<Tkey> root = new Bnode<Tkey>(leaf: true);
		public bool Search(Tkey key) { return root.Search(key); }
		public void Insert(Tkey key)
		{
			(Tkey okey, Bnode<Tkey> overflow_node) = root.Insert(key);
			if (overflow_node != null)
			{	// увеличиваем высоту дерева "вверх"
				var old_root = root;
				root = new Bnode<Tkey>(false) { Count = 1 };
				(root.Keys[0], root.Kids[0], root.Kids[1]) = (okey, old_root, overflow_node);
			}
		}
	}
}