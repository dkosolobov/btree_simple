using System;

namespace BtreeCs
{
	class Bnode<Tkey> where Tkey : IComparable<Tkey>
	{
		const int b = 16;   // sizeof(key) * b = ~64 (длина кеш линии)
		public byte Count = 0;
		public Tkey[] Keys = new Tkey[2 * b + 1];  // Keys[2*b] не используется (но нужен для упрощения реализации)
		public Bnode<Tkey>[] Kids = null;  // Kids[2*b+1] не используется (но нужен для упрощения реализации)
		public Bnode(bool leaf) => Kids = leaf ? null : new Bnode<Tkey>[2 * b + 2]; 

		private int GetKeyPosition(Tkey key)
		{
			int pos = 0;  // Array.FindIndex и Array.BinarySearch заметно медленнее
			while (pos < Count && Keys[pos].CompareTo(key) < 0)
				pos++;    // на коротких массивах линейный поиск быстрее бинарного
			return pos;
		}

		public bool Contains(Tkey key)
		{
			int pos = GetKeyPosition(key);
			if (pos < Count && Keys[pos].Equals(key))
				return true;
			return Kids != null && Kids[pos].Contains(key);
		}

		private (Tkey, Bnode<Tkey>) InsertAtPos(int pos, Tkey key, Bnode<Tkey> nodeAfterKey)
		{
			Array.Copy(Keys, pos, Keys, pos + 1, Count - pos);
			Keys[pos] = key;  // "раздвигаем" Keys и Kids, чтобы вставить key и nodeAfterKey
			if (Kids != null)
			{
				Array.Copy(Kids, pos + 1, Kids, pos + 2, Count - pos);
				Kids[pos + 1] = nodeAfterKey;
			}
			if (++Count <= 2 * b)
				return (default, null);
			return Split();
		}

		private (Tkey, Bnode<Tkey>) Split()
		{
			var median = Keys[b];     // ключ Keys[b] пойдёт на уровень выше;
			var split = new Bnode<Tkey>(Kids == null) { Count = b };
			Array.Copy(Keys, b + 1, split.Keys, 0, b);
			if (Kids != null)
				Array.Copy(Kids, b + 1, split.Kids, 0, b + 1);
			Count = b;  // в узле было 2b+1 ключей
			return (median, split);
		}

		public (Tkey, Bnode<Tkey>) Insert(Tkey key)
		{
			int pos = GetKeyPosition(key);
			Bnode<Tkey> overflow = null;
			if (Kids != null)
				(key, overflow) = Kids[pos].Insert(key);
			if (Kids == null || overflow != null)
				return InsertAtPos(pos, key, overflow);
			return (default, null);
		}
	}

	class BtreeSimple<Tkey> where Tkey : IComparable<Tkey>
	{
		private Bnode<Tkey> root = new Bnode<Tkey>(leaf: true);
		public bool Contains(Tkey key) => root.Contains(key);
		public void Add(Tkey key)
		{
			(Tkey okey, Bnode<Tkey> overflow) = root.Insert(key);
			if (overflow != null)
			{	// увеличиваем высоту дерева "вверх"
				var oldRoot = root;
				root = new Bnode<Tkey>(false) { Count = 1 };
				(root.Keys[0], root.Kids[0], root.Kids[1]) = (okey, oldRoot, overflow);
			}
		}
	}
}