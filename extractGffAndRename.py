import argparse
import csv

def extract_and_rename_gff(gff_in, gff_out, replace_file):
    
    replacements = {}
    with open(replace_file, 'r') as replace_f:
        reader = csv.reader(replace_f, delimiter='\t')
        for row in reader:
            old_name, new_name = row
            replacements[old_name] = new_name

    
    with open(gff_in, 'r') as in_f, open(gff_out, 'w') as out_f:
        for line in in_f:
            if line.startswith('#'):
                continue

            
            fields = line.strip().split('\t')
            attr_str = fields[8]

            
            gene_name = None
            for old_name, new_name in replacements.items():
                if old_name in attr_str:
                    gene_name = old_name
                    break

            if gene_name is not None:
                
                new_attr_str = attr_str
                for old_name, new_name in replacements.items():
                    new_attr_str = new_attr_str.replace(f"={old_name}", f"={new_name}")

                
                fields[8] = new_attr_str
                out_f.write('\t'.join(fields) + '\n')

def main():
    parser = argparse.ArgumentParser(description="Extract annotation information from a GFF file and rename genes.")
    parser.add_argument("--gff_in", required=True, help="Input GFF file.")
    parser.add_argument("--gff_out", required=True, help="Output GFF file with renamed genes.")
    parser.add_argument("--replace", required=True, help="Tab-separated file containing old and new gene names.")

    args = parser.parse_args()
    extract_and_rename_gff(args.gff_in, args.gff_out, args.replace)

if __name__ == "__main__":
    main()
