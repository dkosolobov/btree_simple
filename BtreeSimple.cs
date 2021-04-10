using System;

namespace BtreeCs
{
	class BtreeSimple<Tkey> where Tkey : IComparable<Tkey>
	{
		private Bnode<Tkey> root = new Bnode<Tkey>(leaf: true);

		public bool Contains(Tkey key) => root.Contains(key);

		public void Add(Tkey key)
		{
			(Tkey overkey, Bnode<Tkey> overflow) = root.Insert(key);
			if (overflow == null)
				return;
			var oldRoot = root; // увеличиваем высоту дерева "вверх"
			root = new Bnode<Tkey>(leaf: false) { Count = 1 };
			root.Keys[0] = overkey; 
			(root.Kids[0], root.Kids[1]) = (oldRoot, overflow);
		}
	}

	class Bnode<Tkey> where Tkey : IComparable<Tkey>
	{
		const int b = 16;   // sizeof(key) * b = ~64 (длина кеш линии)
		public int Count = 0;
		public Tkey[] Keys = new Tkey[2 * b + 1];  // Keys[2*b] не используется, но нужен для упрощения реализации
		public Bnode<Tkey>[] Kids = null;        // Kids[2*b+1] не используется, но нужен для упрощения реализации
		public Bnode(bool leaf) => Kids = leaf ? null : new Bnode<Tkey>[2 * b + 2]; 

		public bool Contains(Tkey key)
		{
			int i = GetKeyPosition(key);
			return (i < Count && Keys[i].Equals(key)) || (Kids != null && Kids[i].Contains(key));
		}

		public (Tkey, Bnode<Tkey>) Insert(Tkey key)
		{
			int i = GetKeyPosition(key);
			if (Kids == null)
				return InsertAt(i, key, null);
			(Tkey overkey, Bnode<Tkey> overflow) = Kids[i].Insert(key);
			if (overflow == null)
				return (default, null);
			return InsertAt(i, overkey, overflow);
		}

		private int GetKeyPosition(Tkey key)
		{
			int i = 0;  // Array.FindIndex и Array.BinarySearch заметно медленнее
			while (i < Count && Keys[i].CompareTo(key) < 0)
				i++;    // на коротких массивах линейный поиск быстрее бинарного
			return i;
		}

		private (Tkey, Bnode<Tkey>) InsertAt(int i, Tkey key, Bnode<Tkey> keyNode)
		{
			Array.Copy(Keys, i, Keys, i + 1, Count - i);
			Keys[i] = key;  // раздвигаем Keys и Kids, чтобы вставить key и keyNode
			if (keyNode != null)
			{
				Array.Copy(Kids, i + 1, Kids, i + 2, Count - i);
				Kids[i + 1] = keyNode;
			}
			if (++Count <= 2 * b)
				return (default, null);
			return Split();
		}

		private (Tkey, Bnode<Tkey>) Split()
		{
			var split = new Bnode<Tkey>(Kids == null) { Count = b };
			Array.Copy(Keys, b + 1, split.Keys, 0, b);
			if (Kids != null)
				Array.Copy(Kids, b + 1, split.Kids, 0, b + 1);
			Count = b;
			return (Keys[b], split);  // Keys[b] - это медиана Keys[0..2*b]
		}
	}
}
